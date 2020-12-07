## Laboratory Assignment #8: HADOOP VS MPI



For this lab we would be creating 1 namenode and 3 datanode (so 4 instances in total). (google for the naming convention of Hadoop). Go through below links for detailed understanding of Hadoop, containers and map-reduce architecture. (you will also need to implement same program mentioned in document below on MPI also). So, start earlier on doing lab.

* https://en.wikipedia.org/wiki/MapReduce 
* https://hadoop.apache.org/docs/r2.10.1/hadoop-project-dist/hadoop-hdfs/HdfsDesign.html 
* https://hadoop.apache.org/docs/r2.10.1/hadoop-yarn/hadoop-yarn-site/YARN.html 
* https://hadoop.apache.org/docs/r2.10.1/hadoop-mapreduce-client/hadoop-mapreduce-client-core/MapReduceTutorial.html

Select **Ubuntu Server 16.04 LTS (HVM)** image on x86 and configure the instance with only one interface it being elastic ip interface. Instance type should be of t2.medium or any instance with two vcpus and at least 4 GB of ram and 12 GB of hard drive space (you may try out this in student account to reduce cost). In your selected VPC you have to enable **DNS Hostname** from the VPC tab. For the only subnet attached to the VPC you have to enable **“auto-assign ipv4”** under “Modify auto assign public ipv4 address”. Moreover, you have to “auto assign public ip” set to **“use subnet setting (enable) **. Assign network interface IP statically from within the AWS. These options would automatically assign you a public ip since we have only one network interface per instance. (elastic ip not required)

![](img/lab8-f20.001.png)

![](img/lab8-f20.002.png)

![](img/lab8-f20.003.png)

create a security group with rules to allow all kind of traffic within the private subnet and to your global ip only. (my elastic ip subnet was 10.0.0.0/24 and global ip 104.32.154.67)

![](img/lab8-f20.004.png)

Ensure that all the instances you have created uses same private key for ssh. We will be using only **ubuntu** user account for logging in through ssh.

After logging in, execute below commands on all of the instances:

```
- sudo cp /etc/ssh/ssh_host_rsa_key ~/.ssh/id_rsa
- sudo cp /etc/ssh/ssh_host_rsa_key.pub ~/.ssh/id_rsa.pub (change to ubuntu@ in last )
- sudo cat~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys (change to ubuntu@ in last)
- sudo chmod 777 ~/.ssh/id_rsa
- sudo chmod 777 ~/.ssh/id_rsa.pub
```

![](img/lab8-f20.005.png)

Ensure that in all remote hosts, the authorised\_key file is modified to include public key of the namenode (namenode -> where you will be running Hadoop commands) and it contains ubuntu@ for the host being added to it. My master node ip here was of 10.0.0.247. (these steps are same as the steps for establishing passwordless ssh connectivity as in previous labs).

![](img/lab8-f20.006.png)

Now install java on all the instances by:

```
- sudo apt-get update
- sudo apt install default-jre
- sudo apt-get install default-jdk
- cd ~/
```

Now download hadoop by executing:

```
- wget https://mirrors.koehn.com/apache/hadoop/common/hadoop-2.10.1/hadoop-2.10.1.tar.gz
- sudo tar xzf hadoop-2.10.1.tar.gz
- sudo mv hadoop-2.10.1 /usr/local/hadoop
- sudo chown -R ubuntu /usr/local/hadoop/
```

Edit ~/.profile file with below contents added as shown in figure:

```
- export JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-amd64
- export HADOOP_HOME=/usr/local/hadoop 
- PATH="$HOME/bin:$HOME/.local/bin:$JAVA_HOME/bin:$HADOOP_HOME/bin:$PATH"
```

![](img/lab8-f20.007.png)

Then execute:

- source ~/.profile

This results in environment variables of your current ssh session to be updated. When you login later over ssh, ubuntu automatically source this file.

Edit following files under “/usr/local/hadoop/etc/hadoop/” for all the instances (namenode + datanodes).

### 1. hadoop-env.sh

replace "export JAVA_HOME=" with "export JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk- amd64"

![](img/lab8-f20.008.png)

### 2. core-site.xml

Edit it with the below content ( remove: <configuration> </configuration> and update with below contents)

```
<configuration>

<property>

<name>fs.default.name</name>

<value>hdfs://<namenode_private_IP> :9000</value>

</property>

<property>

<name>hadoop.tmp.dir</name>

<value>/home/ubuntu/hadooptmp</value>

<description>A base for other temporary directories.</description>

</property>

</configuration>
```

![](img/lab8-f20.009.png)Change the **namenode\_private\_IP** to eth0 interface ip of the namemode for all of the instances (namenode + datanodes). (this ip should not change in-between instance reboots, so better to assign static in aws network interface creation phase)

Also **create "/home/ubuntu/hadooptmp"** (as it's specified in the above configuration for temp directory for hdfs)

**core-site.xml** contains the configuration settings for Hadoop Core (eg I/O) that are common to HDFS and MapReduce. It also informs Hadoop daemon where NameNode (the master) runs in the cluster. So each node(instance) must have this file completed.

### 3. hdfs-site.xml

Edit with below content: ( remove: <configuration> </configuration> and update with below contents)

```
<configuration>

<property>

<name>dfs.replication</name> <value>3</value>

</property>

<property>

<name>dfs.namenode.name.dir</name>

<value>/usr/local/hadoop/hdfs/namenode</value> </property>

<property>

<name>dfs.namenode.data.dir</name>

<value>/usr/local/hadoop/hdfs/datanode</value> </property>

</configuration>
```
![](img/lab8-f20.010.png)

### 4. mapred-site.xml.template

First rename mapred-site.xml.template to mapred-site.xml.

```
mv /usr/local/hadoop/etc/hadoop/mapred-site.xml.template /usr/local/hadoop/etc/hadoop/mapred-site.xml
```

Then put below contents in it.

```
<configuration>

<property>

<name>mapreduce.framework.name</name> <value>yarn</value>

</property>

</configuration>
```
![](img/lab8-f20.011.png)

### 5. yarn-site.xml

Edit with below content: ( remove: <configuration> </configuration> and update with below contents)

```
<configuration>

<property>

<name>yarn.nodemanager.aux-services</name> <value>mapreduce_shuffle</value>

</property>

<property>

<name>yarn.nodemanager.auxservices.mapreduce.shuffle.class</name>

<value>org.apache.hadoop.mapred.ShuffleHandler</value> </property>

<property>

<name>yarn.resourcemanager.hostname</name> <value><namenode_private_IP> </value>

<description>The hostname of the Resource Manager.</description> </property>

</configuration>
```

Give namenode eth0 interface ip in <namenode_private_IP> for all of the instances (namenode + datanodes).

Replicate the above configurations in **3 datanode** instances also.

Now log into each of the **datanode** instances. Create:

1. empty master files using "touch /usr/local/hadoop/etc/hadoop/masters"
2. vi /usr/local/hadoop/etc/hadoop/slaves

Specify eth0 ip of the current datanode instance (where you are executing the vi command) in "/usr/local/hadoop/etc/hadoop/slaves". Remove all other contents. (only one ip should be present)

![](img/lab8-f20.012.png)

My one of the datanode’s IP was 10.0.0.125.

Now in the namenode instance. Edit

1. vi /usr/local/hadoop/etc/hadoop/masters

Give eth0 interface ip of the namenode in the file.

![](img/lab8-f20.013.png)

My master node ip was 10.0.0.247.

2. vi /usr/local/hadoop/etc/hadoop/slaves

Give eth0 IP of all of the 3 data-nodes in this file. Remove all other contents. (add line by line each IP)

![](img/lab8-f20.014.png)

Now perform from master-node below commands:

- ssh -v -x ubuntu@<IP>

to all the 3 data-nodes from namenode. If it asks for a message confirmation type yes. This should establish a passwordless ssh connectivity to remote host. Type exit to bring back control to the source host. (Only proceed further if you are able to do passwordless ssh access from namenode to all of the datanodes).

Execute below command only on namenode to create hdfs file system (directory is specified in hdf- site.xml):

- /usr/local/hadoop/bin/hdfs namenode -format

Now we will start hdfs and yarn daemons of Hadoop. Hdfs is responsible for maintaining hdfs distributed file system and yarn for containers and resource management.

```
- /usr/local/hadoop/sbin/start-dfs.sh

- /usr/local/hadoop/sbin/start-yarn.sh
```

If it asks for yes/no message, type yes and press enter (it’s self-logging in namenode instance using localhost).

For debugging purposes, logs are located under "/usr/local/hadoop/logs" in datanodes and namenodes.If you face any issues first check the logs under these folder and make an inference what could be the issue by googling.

Execute below command to verify that datanode and namenode is up and configured:

- hdfs dfsadmin -report

This should show your namenode and 3 datanodes.

![](img/lab8-f20.015.png)

Now from the namenode, execute below sample program:

- hadoop jar /usr/local/hadoop/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.10.1.jar pi 10 1000

If above program ran successfully you should be seeing an output like below:

![](img/lab8-f20.016.png)

You can access hadoop Master dashboard from: http://<namenode instance public ip>:8088/cluster

![](img/lab8-f20.017.png)

datanode instance access from:

http://<datanode instance public ip>:8042/cluster

![](img/lab8-f20.018.png)

You can view hadoop filesystem with webgui by follwoing below link:

http://<namenode public ip>:50070

![](img/lab8-f20.019.png)

You can explore more programs under "hadoop jar /usr/local/hadoop/share/hadoop/mapreduce/hadoop- mapreduce-examples-2.10.1.jar"

ð If you get error while clicking some text under page, replace the private ip in http link with corresponding instance public ip .

Debugging Tips:

use command "jps " to view java running processes on each instances.

"netstat -a -p | grep -i port" and then "kill pid". For releasing the port. (To troubleshoot port error obtained from logs inside for hdfs and yarn daemons)

hadoop job -kill <job_id> --> to kill job (from namenode)

Now we will be discussing a technique in which you can use python to write your map and reduce functions. (normally in hadoop it's in java as you have seen in above examples). We will be using streaming api support of Hadoop to achieve this. (even c/c++ program can also be done in this way)

Install python on all datanodes and namenode.

- sudo apt-get install python

Here we will be implementing a word count map-reduce program. It reads text files and counts how often words occur. The input is text files and the output is text files, each line of which contains a word and the count of how often it occurred, separated by a tab.

Create a mapper and reduce function in python as explained below (under home directory).

1. Create mapper.py which would be used for map function with below contents. Also change permission to 777 (by executing chmod 777 mapper.py):

```
#!/usr/bin/env python """mapper.py"""

import sys

# input comes from STDIN (standard input)

for line in sys.stdin:

# remove leading and trailing whitespace

line = line.strip()

# split the line into words

words = line.split()

# increase counters

for word in words:

# write the results to STDOUT (standard output);
# what we output here will be the input for the
# Reduce step, i.e. the input for reducer.py



# tab-delimited; the trivial word count is 1

print '%s\t%s' % (word, 1)
```

2. Create reducer.py for reduce functionality of map-reduce with below contents. Also change permission to 777 (by executing chmod 777 reduce.py):

```
#!/usr/bin/env python reducer.py

from operator import itemgetter import sys

current_word = None current_count = 0 
word = None

# input comes from STDIN

for line in sys.stdin:

# remove leading and trailing whitespace line = line.strip()
# parse the input we got from mapper.py word, count = line.split('\t', 1)
# convert count (currently a string) to int try:

count = int(count)

except ValueError:

- count was not a number, so silently
- ignore/discard this line

continue

- this IF-switch only works because Hadoop sorts map output
- by key (here: word) before it is passed to the reducer if current_word == word:

current_count += count

else:

if current_word:

- write result to STDOUT

print '%s\t%s' % (current_word, current_count) current_count = count

current_word = word

- do not forget to output the last word if needed! if current_word == word:

print '%s\t%s' % (current_word, current_count)
```


* Download an input dataset which is a sample text file for testing map-reduce program by executing:

```
wget https://www.gutenberg.org/files/20417/20417-8.txt
```

* Now run:

```
cat 20417-8.txt | ./mapper.py | sort -k1,1 | ./reducer.py
```

It should produce output like below: (ie <text> <word count>) (this is manual way to simulate operations which map-reduce does in background, so that the program can be tested before starting it as map-reduce job)

![](img/lab8-f20.020.png)

Now download 3 input text file for passing input to map reduce job. First create a directory under home and cd to that and download the txt files using wget:

```
- mkdir ~/test_txt
- cd ~/test_txt
- wget http://www.gutenberg.org/files/5000/5000-8.tx
- wget http://www.gutenberg.org/files/4300/4300-0.txt
- wget http://www.gutenberg.org/cache/epub/20417/pg20417.txt
```

Execute below command to copy from local file system to hadoop filesystem.

```
- hdfs dfs -copyFromLocal ~/test_txt /user/ubuntu/gutenberg

For Viewing files under hdfs execute:

- hdfs dfs -ls /user/ubuntu
```

![](img/lab8-f20.021.png)

This should show a gutenberg folder as show above. Same thing should be visible from web gui access also on port 50070.

4. Now start map-reduce job by:

- hadoop jar /usr/local/hadoop/share/hadoop/tools/lib/hadoop- streaming-2.10.1.jar -file /home/ubuntu/mapper.py -mapper /home/ubuntu/mapper.py -file /home/ubuntu/reducer.py -reducer /home/ubuntu/reducer.py -input /user/ubuntu/gutenberg/* -output /user/ubuntu/gutenberg-output

If everything was succesfull, you would be seeing a directory created at /user/ubuntu/gutenberg-output and contents visible under it.

![](img/lab8-f20.022.png)

Here I have specified output as /user/ubuntu/gutenberg-output5.

===> Now modify the above program to find the most and least frequent words. (if multiple words with same frequency, print all such occurrences) and append it to last of the generated text output along

with normal output. Execute new map-reduce job with your new modified program. (you might want to delete previous file at output path, google for the Hadoop command to remove file)

Perform below command to verify that output file is created: (as shown below)

- hdfs dfs -ls /user/ubuntu/gutenberg-output/

![](img/lab8-f20.023.png)

Verify content by executing:

- hdfs dfs -cat /user/ubuntu/gutenberg-output/part-00000 (here most and least is not shown)

![](img/lab8-f20.024.png)

Now we optimize mapper.py and reducer.py with below contents, which uses iterators and generators. (for improved performance)

1. mapper.py

```
#!/usr/bin/env python

"""A more advanced Mapper, using Python iterators and generators."""

import sys

def read_input(file):

for line in file:

# split the line into words yield line.split()

def main(separator='\t'):

# input comes from STDIN (standard input)

data = read\_input(sys.stdin)

for words in data:

# write the results to STDOUT (standard output);
# what we output here will be the input for the
# Reduce step, i.e. the input for reducer.py



- tab-delimited; the trivial word count is 1

for word in words:

print '%s%s%d' % (word, separator, 1)

if __name__ == "__main__":

main()
```

2. reducer.py

```
#!/usr/bin/env python

"""A more advanced Reducer, using Python iterators and generators."""

from itertools import groupby from operator import itemgetter import sys

def read_mapper_output(file, separator='\t'):

for line in file:

yield line.rstrip().split(separator, 1)

def main(separator='\t'):

- input comes from STDIN (standard input)

data = read_mapper_output(sys.stdin, separator=separator)

- groupby groups multiple word-count pairs by word,
- and creates an iterator that returns consecutive keys and their group:
- current_word # string containing a word (the key)
- group - iterator yielding all ["&lt;current_word&gt;", "&lt;count&gt;"] items for current_word, group in groupby(data, itemgetter(0)):

try:

total_count = sum(int(count) for current_word, count in group)

print "%s%s%d" % (current_word, separator, total_count)

except ValueError:

- count was not a number, so silently discard this item

pass

if __name__ == "__main__":

main()
```

Now download input dataset from:

- wget http://www.i3s.unice.fr/~jplozi/hadooplab_lsds_2015/datasets/gutenber g-1G.txt.gz
- gunzip gutenberg-1G.txt.gz

Then copy this file to hadoop hdfs by executing:

- hdfs dfs -copyFromLocal ~/<filepath> /user/ubuntu/gutenberg2

===> 1. Then run map-reduce job with your modified code with this input dataset by executing: (I will be using this input dataset for grading purpose with your python changes present on map and reduce functions and it should use iterators and generators)( your new .py files should be uploaded along with report or video submission))

```
- hadoop jar /usr/local/hadoop/share/hadoop/tools/lib/hadoop-

streaming-2.10.1.jar -file /home/ubuntu/mapper.py -mapper

/home/ubuntu/mapper.py -file /home/ubuntu/reducer.py -reducer

/home/ubuntu/reducer.py -input /user/ubuntu/gutenberg2/gutenberg-1G.txt -output /user/ubuntu/gutenberg-output2
```

===> 2. Now replicate the above program on MPI with input dataset as gutenberg-1G.txt ( implement word count with max, min frequency count of words). ( will use the gutenberg-1G.txt dataset for grading, your new c files should be uploaded along with report or video submission).

Now compare performance of your MPI and hadoop word count program . What is your observation?

One important point to note is that when you are comparing performance of MPI and hadoop you should be careful to use same instance type and same number of instance nodes for all the jobs. (master and worker nodes).

==>3. Now you have to write a new program from scratch which counts frequency of ascii characters instead of words from input text file (use gutenberg-1G.txt as input dataset). Print all the ascii character encountered along with frequency by processing input dataset and identify also characters occurring frequently and characters occurring the least . (print multiple occurrences if they have same frequency). Implement this new program on hadoop and MPI . Run and compare performance of the program implementation on MPI and Hadoop. (if possible, plot it as bar graph with x-axis denoting ascii character and y-axis frequency of that character)

So, in total you have two programs to implement from scratch for hadoop and MPI.

- Word count problem along with with max, min word display.
- Displaying frequency of ascii characters along with identification of max, min character.

I will be relying on output of the above 1,2,3 program of map-reduce on Hadoop and MPI for grading purpose.

**REFERENCES:**

* https://en.wikipedia.org/wiki/MapReduce 
* https://hadoop.apache.org/docs/r2.10.1/hadoop-project-dist/hadoop-hdfs/HdfsDesign.html 
* https://hadoop.apache.org/docs/r2.10.1/hadoop-yarn/hadoop-yarn-site/YARN.html 
* https://hadoop.apache.org/docs/r2.10.1/hadoop-mapreduce-client/hadoop-mapreduce-client-core/MapReduceTutorial.html
* https://www.tutorialspoint.com/hadoop/hadoop_mapreduce.htm https://hadoop.apache.org/docs/r2.10.1/hadoop-project-dist/hadoop-common/ClusterSetup.html 
* https://www.michael-noll.com/tutorials/writing-an-hadoop-mapreduce-program-in-python/ 
* https://blog.gaelfoppolo.com/lets-try-hadoop-on-aws-13a23e641490 
* https://developer.ibm.com/articles/l-pycon/
