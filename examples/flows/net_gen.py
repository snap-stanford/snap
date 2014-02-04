import sys
import random
assert len(sys.argv) >= 4
nIdBound = int(sys.argv[1])
iterBound = int(sys.argv[2])
capBound = int(sys.argv[3])
fileName = 'flow_{}_{}_{}.txt'.format(nIdBound, iterBound, capBound);
print 'Generating random flow network to file ' + fileName
print '  Max node ID:', nIdBound
print '  Max edge capacity:', capBound
print 'Running for',iterBound,'iterations...'
outfile = open(fileName, 'w')
random.seed()
edges = set()
nodes = set()
for i in range(1, iterBound):
  edge = (random.randint(1, nIdBound), random.randint(1, nIdBound))
  if edge not in edges and edge[0] != edge[1]:
    nodes.add(edge[0])
    nodes.add(edge[1])
    edges.add(edge)
    outfile.write('{} {} {}\n'.format(edge[0],edge[1],random.randint(0, capBound)))
print '\nGenerated graph with',len(nodes),'nodes and',len(edges),'edges'
