
import java.io.IOException;
import java.util.*;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class auto_cost_summary {

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

    public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, Text> {
	public void map(LongWritable key, Text value, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {
	    output.collect(value, new Text("1"));
	}
    }

    public static class Reduce extends MapReduceBase implements Reducer<Text, Text, Text, Text> {
	public void reduce(Text key, Iterator<Text> values, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {

	    int count = 0;
	    while (values.hasNext()) {
		values.next();
		count++;
	    }

	    output.collect(key, new Text(Integer.toString(count)));
	}
    }

    public static void main(String[] args) throws Exception {

	if (args.length != 2) {
	    System.out.println("> hadoop jar auto_cost_summary auto_cost_summary $INPUT_PATH $OUT_PATH");
	    return;
	}

	System.out.println("$INPUT_PATH=" + args[0]);
	System.out.println("$OUT_PATH=" + args[1]);

	JobConf conf = new JobConf(auto_cost_summary.class);
	conf.setJobName("auto_cost_summary");

	conf.setMapperClass(Map.class);
	conf.setReducerClass(Reduce.class);

	conf.setOutputKeyClass(Text.class);
	conf.setOutputValueClass(Text.class);

	FileInputFormat.setInputPaths(conf, new Path(args[0]));
	FileOutputFormat.setOutputPath(conf, new Path(args[1]));

	conf.setBoolean("mapreduce.input.fileinputformat.input.dir.recursive", true);
	JobClient.runJob(conf);
    }
}


