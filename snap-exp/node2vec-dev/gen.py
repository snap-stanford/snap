import networkx as nx
G=nx.erdos_renyi_graph(100,0.1)
nx.write_edgelist(G.to_undirected(),"graph.txt",data=False)
