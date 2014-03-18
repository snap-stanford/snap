#!/usr/bin/python
import MySQLdb

db = MySQLdb.connect(host="localhost",
                     user="enron",
                     passwd="enron",
                     db="enronsql")
cur = db.cursor()
cur.execute("SELECT * FROM mailgraph")
with open("enronData.txt", "w") as f:
    for row in cur.fetchall():
        if row[0] != row[1]:
            f.write("%d\t%d\n" % (row[0], row[1]))
