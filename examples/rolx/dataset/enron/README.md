This database is downloaded from UCB Enron analysis page:
http://bailando.sims.berkeley.edu/enron_email.html

Text excerpted from the website
------------------------------------------------------
A database representation(219 MB compressed) of the Enron email collection, built by Andrew Fiore and Jeff Heer, containing the enron email messages. This version contains many but not all of the tables used in the search tool, as well as special tables to be used with the Enronic visualization tool. Andrew did a substantial amount of processing on the contents of the database to remove duplicates, normalize names, and so on. This has been tested only on mysql.

The database itself is from
http://bailando.sims.berkeley.edu/enron/enron.sql.gz

How the data set is used here
------------------------------------------------------
Run RolX:
1. The whole dataset (enron.sql) is loaded to MySQL.
   There are some syntax errors that may cause import problems,
   just Google it and make corresponding change in enron.sql.
2. Use genEnron.py to generate the email link file (enronData.txt) from the mailgraph table.
3. Run RolX on enronData.txt

Make sense out of result:
1. In MySQL, export the people table to people.csv.
2. From people.csv, generate node-name mapping (node_name.txt)
3. From job.pdf generate the name-job mapping (name_job.txt)
4. From node-job mapping and name-job mapping,
   use genNodeToJob.py to generate node-job mapping (node_job.txt)
5. Use node-job mapping to make sense out of each node's role found by RolX.
