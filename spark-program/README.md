Laboratory 9: Spark Programming

Apache Spark is a powerful distributed computing framework for efficiently processing very large datasets. You will explore its libraries using the PySpark API in both a standalone and distributed environment. (start on this early as you have to run your program on **Hadoop**, **MPI** and **spark** with varying number of worker nodes)

## **1. PySpark**

We will first practice using Spark in local mode on a single-node machine. The following instructions only cover installation on an EC2 instance. If you are already familiar with using Docker containers, you may do this part on your own laptop or computer.|

You may refer: [https://aws.amazon.com/emr/pricing/ ](https://aws.amazon.com/emr/pricing/)and select an instance with reduced cost. One you may use is c5.large instance type.

Create an EC2 instance using the **Amazon Linux 2 AMI** and select **c5.large** for your instance type.
|Make sure to modify your instance’s security group inbound rules so that **port 8888** is accessible by your IP address (tcp) . You would be needing only one network interface and accordingly select auto public ip with the steps mentioned in lab8. Specify hdd space as 12 GB. (your username in Amazon Linux AMI is ec2-user) (you will need to perform this from AWS free tier account or from student account).(you should modify security group and network ACL rule in your VPC to allow public interne access while downloading the package).

Run the following commands to install Docker onto your instance and logout of your SSH session.|

```
sudo yum update -y
sudo yum install -y docker
sudo service docker start
sudo usermod -a -G docker ec2-user
exit
```

SSH tunnel into your instance with the -L flag as shown below and run the subsequent commands to install the Docker image containing PySpark and Jupyter Notebook.

```
## Connect localhost port 8888 to instance port 8888
ssh -i myKey.pem ec2-user@XXXX.compute.amazonaws.com -L 8888:127.0.0.1:8888

## Install Docker image with PySpark Notebook|
docker run -v ~/work:/home/jovyan/work -d -p 8888:8888 jupyter/pyspark-notebook

## Allow preserving Jupyter notebook
sudo chown 1000 ~/work

## Install tree to see our working directory next
sudo yum install -y tree

```
Run the following command to get the name of your container.

```
docker ps
```
![](img/lab9-f20.001.png)

In this case, my container’s name is “agitated\_mcclintock”. Use the Docker logs to get the link to access your Jupyter Notebook running PySpark by running the following command.

```
docker logs “name of container with no quotes”
```

Copy the displayed link into your browser and create a new Python 3 notebook.

![](img/lab9-f20.002.png)

Follow the tutorial linked below starting from section “Spark Context” to get familiar with the PySpark API. **The dataset that is used has been altered since the tutorial’s writing, changing several features and breaking certain functions. Additionally, there are several typos that you will need to fix.** When you’re finished, download your notebook and make sure to **TERMINATE** your instance to avoid additional costs. (You might want to google for how to use jupyter notebook instance and how to run the program step by step and re-execution)

[https://www.guru99.com/pyspark-tutorial.html](https://www.guru99.com/pyspark-tutorial.html)

For a better fundamental understanding of how Spark and its API work, it is recommended that you create an edX account and watch all videos from Week 2, Lecture 2b of the BerkeleyX course “Scalable Machine Learning”.

[https://courses.edx.org/courses/BerkeleyX/CS190.1x/1T2015/course/](https://courses.edx.org/courses/BerkeleyX/CS190.1x/1T2015/course/)

After completing the tutorial, download the “Smartphone-Based Recognition of Human Activities and Postural Transitions Data Set” from the UCI Machine Learning Repository. This dataset contains accelerometer and gyroscope data from smartphones of people doing various activities. You can read more about the dataset and download it from the below link.

[https://archive.ics.uci.edu/ml/datasets/Smartphone-Based+Recognition+of+Human+Activities+and+Postural+Transitions](https://archive.ics.uci.edu/ml/datasets/Smartphone-Based+Recognition+of+Human+Activities+and+Postural+Transitions)

Complete the following tasks on the dataset:

- Create a machine learning model using any algorithm not used in the tutorial from Spark’s MLLib.
- Report the training accuracy, test accuracy, and test F1-score for your model.

## **2. AWS Elastic MapReduce**

When you run Spark in local mode, it will allocate tasks to local threads instead of worker nodes. Spark is a distributed computing framework and was not primarily designed to be used on a single-node machine. Here, we will practice running a Spark job on a real cluster using AWS Elastic MapReduce.

First, upload the smartphone data from the previous part into an S3 bucket.

You may refer: [https://aws.amazon.com/emr/pricing/](https://aws.amazon.com/emr/pricing/) and select an instance with reduced cost. You may use c5.xlarge instance type.

Go to EMR in the AWS console and click on the “Notebook” tab. Create a two-node cluster of instance type c5.xlarge (or m5.xlarge ->if you face issues). EMR will create several EC2 instances of your selected type (as specified in the instance count) and automatically configure your cluster manager. (you may try these out from student account)

Wait for your cluster to start and your notebook to attach to the cluster. This can take anywhere from 5- 10 minutes. Running a cluster in EMR is very expensive, so do not leave it idle and start using your notebook immediately when it becomes ready.

Open your notebook in JupyterLab, select your created notebook, and select PySpark for your kernel. Run the code from model you made in the previous part (i.e on Smartphone-Based Recognition of Human Activities and Postural Transitions Data Set). In EMR, your **SparkContext object is already created** so you don’t have to declare it. You can reference it using “sc” as usual. You will also have to **reference files from S3** instead of a local file directory.

When you’re finished, **TERMINATE** your cluster in the “Clusters” tab to avoid extra charges. You will certainly **accumulate huge charges** if this is not done properly. Your notebook is saved in an S3 bucket (which you specified while creating), which you can download and delete later if you choose. (you might check EC2 instance tab and terminate instances created by EMR manually)

3. Custom Spark Program

Replicate functionally equivalent system as the codes you implemented using Hadoop and MPI on the previous Laboratory assignment with Spark. (i.e word count and frequency count of character with all the min and max word, character appended to an output file which gets stored in S3 bucket) (first try it on local spark and then on EMR at AWS -> to reduce cost of usage)

(you might want to first start testing with 5 MB Gutenberg file and after making everything work test it on 1 GB Gutenberg file)

[http://www.i3s.unice.fr/~jplozi/hadooplab\_lsds\_2015/datasets/gutenberg-1G.txt.gz](http://www.i3s.unice.fr/~jplozi/hadooplab\_lsds\_2015/datasets/gutenberg-1G.txt.gz)

Also ensure that you **don’t count space** as character for frequency count program and only take ascii characters **within ascii values of range [33 – 122]** and print it as character and not as integer.-> applicable for your previous Hadoop and MPI program also)

![](img/lab9-f20.006.png)

Compare **Spark system against Hadoop and MPI** . Make **a diagram showing scalability performances for different number of nodes** . (so, you might want to try out with different number of worker nodes and execute your program on MPI, Hadoop and Spark on your created cluster and plot a graph by varying number of worker nodes). (**your program should be written in a generic way to work with varying number of worker nodes**). (you may try these out from student account)

Make sure to take precautions for “**AWS data transfer out charges**” with security group, network ACL as well as iptables as mentioned during lab8.

Sources

* https://www.guru99.com/pyspark-tutorial.html 
* https://courses.edx.org/courses/BerkeleyX/CS190.1x/1T2015/course/ 
* https://archive.ics.uci.edu/ml/datasets/Smartphone-Based+Recognition+of+Human+Activities+and+Postural+Transitions 
* https://spark.apache.org/docs/latest/rdd-programming-guide.html
