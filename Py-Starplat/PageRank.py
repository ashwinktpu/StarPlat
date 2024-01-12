def Compute_PR(g, beta, delta, maxIter):
    
    num_nodes = g.num_nodes()
    g.attachNodeProperty(pageRank=1/num_nodes, pageRank_nxt=0)
    iterCount = 0
    
    condition = True
    while condition:
        for v in g.nodes():
            sum = 0.0
            
            for nbr in g.nodes_to(v):
                sum += g.pageRank[nbr] / g.count_outNbrs(nbr)
                
            val = (1-delta)/num_nodes + delta*sum
            
            if g.pageRank[v] >= 0:
                diff += g.pageRank[v] - val
            else:
                diff += val - g.pageRank[v]
                
            g.pageRank_nxt[v] = val
            
        g.pageRank = g.pageRank_nxt.copy()
        iterCount +=1
        
        condition = (diff > beta) and (iterCount < maxIter)