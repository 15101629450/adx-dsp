
import java.io.IOException;
import java.util.*;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class auto_cost_merge {

    public static String ascii_split(String str, int ch) {

	String ret = "";
	char[] cstr = str.toCharArray();

	int n = 0;
	for (int i : cstr) {
	    if(i == ch) return ret;
	    ret += str.charAt(n++);
	}

	return ret;
    }

    public static String time_parse(String str) {

	long timeStr = Long.parseLong(str);
	SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd-HH");
	return simpleDateFormat.format(timeStr);
    }

    public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, Text> {
	public void map(LongWritable key, Text value, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {

	    String line = value.toString();
	    String path = ((FileSplit) reporter.getInputSplit()).getPath().toString();
	    if (path.indexOf("sparkMerge") >= 0) {

		String[] lines	= line.split("\t");
		if (lines.length != 70) return;

		String time	= time_parse(lines[0]);
		String adx	= lines[1];
		String bid	= lines[2];
		String orbid	= lines[3];
		String region	= ascii_split(lines[45], 1);
		String mapid	= lines[67];

		String buffer = "1" + time;
		buffer += "\t" + adx;
		buffer += "\t" + mapid;
		buffer += "\t" + region;
		if (orbid.equals("1")) output.collect(new Text(bid), new Text(buffer));

	    } else if (path.indexOf("odinCounter") >= 0) {

		String[] lines	= line.split("\\|");
		if (lines.length != 18) return;

		String bid	= lines[1];
		String iv	= lines[16];

		String buffer = "2";
		if (iv.equals("1")) output.collect(new Text(bid), new Text(buffer));
	    }
	}
    }

    public static class Reduce extends MapReduceBase implements Reducer<Text, Text, Text, Text> {

	private Text buffer = new Text();
	public void reduce(Text key, Iterator<Text> values, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {

	    boolean bid_status = false;
	    boolean tracker_status = false;
	    while (values.hasNext()) {
		String value = values.next().toString();
		String ch = value.substring(0, 1);

		if (ch.equals("1")) {
		    buffer.set(value.substring(1));
		    bid_status = true;

		} else if (ch.equals("2")) {
		    tracker_status = true;
		}
	    }

	    if (bid_status) {
		if (tracker_status) output.collect(buffer, new Text("1"));
		else output.collect(buffer, new Text("0"));
	    }

	    // System.out.println(key.toString() + "bid_status=" + bid_status + " tracker_status=" + tracker_status);
	}
    }

    public static void main(String[] args) throws Exception {

	if (args.length != 3) {
	    System.out.println("> hadoop jar auto_cost_merge auto_cost_merge $BID_PATH $TRACKER_PATH $OUT_PATH");
	    return;
	}

	System.out.println("$BID_PATH=" + args[0]);
	System.out.println("$TRACKER_PATH=" + args[1]);
	System.out.println("$OUT_PATH=" + args[2]);

	JobConf conf = new JobConf(auto_cost_merge.class);
	conf.setJobName("auto_cost_merge");

	conf.setMapperClass(Map.class);
	conf.setReducerClass(Reduce.class);

	conf.setOutputKeyClass(Text.class);
	conf.setOutputValueClass(Text.class);

	Path[] inputPaths = new Path[2];
	inputPaths[0] = new Path(args[0]);
	inputPaths[1] = new Path(args[1]);

	FileInputFormat.setInputPaths(conf, inputPaths);
	FileOutputFormat.setOutputPath(conf, new Path(args[2]));

	conf.setBoolean("mapreduce.input.fileinputformat.input.dir.recursive", true);
	JobClient.runJob(conf);
    }
}


