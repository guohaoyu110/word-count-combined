# MPI word-count

## Set up 

First create 4 **t2.large** micro instance with ubuntu 16.04 x86 OS and attach elastic ip to it.(if you are in student account, choose an instance which give you at least 8 GB of memory with least cost as mpi require at least 2.2 GB of shared memory to work). Make sure that all of the instances are using same private key for authentication in ssh and are in same subnet under same VPC and are able to ping each other (on non-elastic interface, other steps you have to follow from previous labs like igw addition and routing table updation). Your instance would have two network interfaces. One is used for elastic ip and other used for communication between instances.

Make sure that **elastic ip** is assigned to a subnet different from that of the other interface subnet on each instance. You would have to configure the security group to allow tcp, icmp, udp packets on your private ip subnet range. You can choose private ip in range of 10.0.1.0/24 subnet (all instances attached to this same subnet) and elastic ip in range of 10.0.100.0/24, 10.0.101.0/24, 10.0.102.0/24 and 10.0.103.0/24. Assign private ip while you are creating interface in 10.0.1.0/24 subnet from 10.0.1.10 onwards.

Also modify **/etc/network/interfaces** on all instances to have dhcp set ip on eth1 interface and perform “sudo systemctl restart networking” so that eth1 interface gets IP.