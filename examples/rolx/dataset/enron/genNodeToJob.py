import pprint

def read_node_and_name():
    fileName = 'people.csv'
    nodeName = {}
    with open(fileName, 'r') as f:
        for line in f:
            fields = line.split(',')
            nodeName[int(fields[0].strip())] = fields[1].strip().lower()
    return nodeName

def read_node_job_fields(line):
    fields = line.split(',')
    return fields[0].strip().lower(),\
           fields[1].strip().lower(),\
           fields[2].strip().lower()

def gen_node_and_job(nodeName):
    fileName = 'edited_job.txt'
    nodeJob = {}
    with open(fileName, 'r') as f:
        for line in f:
            if line.startswith('#'):
                continue
            lastName, firstName, job = read_node_job_fields(line)
            candidates = []
            for node, name in nodeName.iteritems():
                if lastName in name:
                    candidates.append(node)
            if len(candidates) == 1:
                nodeJob[candidates[0]] = job
            else:
                for node in candidates:
                    if firstName in nodeName[node]:
                        nodeJob[node] = job
    return nodeJob

if __name__ == '__main__':
   nodeName = read_node_and_name()
   nodeJob = gen_node_and_job(nodeName)
   with open('node_job.txt', 'w') as f:
       for node in sorted(nodeJob.keys()):
           f.write('%d,%s\n' % (node, nodeJob[node]))
