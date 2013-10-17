def ConvertGraph(toutspec, tinspec, *args):
    if toutspec == PUNGraph:
        if type(tinspec) == PUNGraph:
            return ConvertGraph_PUNGraph_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertGraph_PUNGraph_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertGraph_PUNGraph_PNEANet(tinspec, *args)
    if toutspec == PNGraph:
        if type(tinspec) == PUNGraph:
            return ConvertGraph_PNGraph_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertGraph_PNGraph_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertGraph_PNGraph_PNEANet(tinspec, *args)
    if toutspec == PNEANet:
        if type(tinspec) == PUNGraph:
            return ConvertGraph_PNEANet_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertGraph_PNEANet_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertGraph_PNEANet_PNEANet(tinspec, *args)
    return None
def ConvertSubGraph(toutspec, tinspec, *args):
    if toutspec == PUNGraph:
        if type(tinspec) == PUNGraph:
            return ConvertSubGraph_PUNGraph_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertSubGraph_PUNGraph_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertSubGraph_PUNGraph_PNEANet(tinspec, *args)
    if toutspec == PNGraph:
        if type(tinspec) == PUNGraph:
            return ConvertSubGraph_PNGraph_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertSubGraph_PNGraph_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertSubGraph_PNGraph_PNEANet(tinspec, *args)
    if toutspec == PNEANet:
        if type(tinspec) == PUNGraph:
            return ConvertSubGraph_PNEANet_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertSubGraph_PNEANet_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertSubGraph_PNEANet_PNEANet(tinspec, *args)
    return None
def ConvertESubGraph(toutspec, tinspec, *args):
    if toutspec == PUNGraph:
        if type(tinspec) == PUNGraph:
            return ConvertESubGraph_PUNGraph_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertESubGraph_PUNGraph_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertESubGraph_PUNGraph_PNEANet(tinspec, *args)
    if toutspec == PNGraph:
        if type(tinspec) == PUNGraph:
            return ConvertESubGraph_PNGraph_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertESubGraph_PNGraph_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertESubGraph_PNGraph_PNEANet(tinspec, *args)
    if toutspec == PNEANet:
        if type(tinspec) == PUNGraph:
            return ConvertESubGraph_PNEANet_PUNGraph(tinspec, *args)
        if type(tinspec) == PNGraph:
            return ConvertESubGraph_PNEANet_PNGraph(tinspec, *args)
        if type(tinspec) == PNEANet:
            return ConvertESubGraph_PNEANet_PNEANet(tinspec, *args)
    return None










