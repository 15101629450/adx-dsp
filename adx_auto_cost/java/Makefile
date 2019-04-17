

all : merge_compile summary_compile

merge_compile : clean
	javac auto_cost_merge.java
	jar -cvf auto_cost_merge.jar auto_cost_merge*.class
	rm -f auto_cost*.class

summary_compile : clean
	javac auto_cost_summary.java
	jar -cvf auto_cost_summary.jar auto_cost_summary*.class
	rm -f auto_cost*.class

clean :
	rm -f auto_cost*.class
	rm -f auto_cost*.jar
	
