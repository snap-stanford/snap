## README
# Before you start, remember to set the directory this file is in as the
# working directory.
##

## INSTALL / LOAD LIBRARIES
# Install reshape2, ggplot2, and their dependencies if they are not installed
if (!"ggplot2" %in% rownames(installed.packages()) ||
    !"reshape2" %in% rownames(installed.packages())) {
  install.packages("ggplot2")
  install.packages("reshape2")
}

require(reshape2)
require(ggplot2)
require(plyr)
require(scales)

## LOAD EXPERIMENT DATA
runtime <- read.csv("./parallelize_runtime.csv", header = TRUE)
runtime$parallelized <- as.logical(runtime$parallelized)


####################
## TABLE 3 - Comparison between parallelized and unparrlelised version, in numbers
####################
runtime_melted <-
  melt(runtime,
       id.vars = c("network", "parallelized"), 
       measure.vars = c("CT_runtime", "GA_runtime",
                        "CA_runtime")
  )

runtime_CA_summary <- 
  ddply(runtime_melted[runtime_melted$variable == "CA_runtime", ], 
        c("network", "parallelized"), 
        summarise, count = length(value), mean = mean(value), var = var(value)
  )

runtime_overall <-
  data.frame(
    network = runtime$network,
    parallelized = runtime$parallelized,
    overallRuntime = runtime$CT_runtime + runtime$GA_runtime +
      runtime$CA_runtime
  )

runtime_overall_summary <-
  ddply(runtime_overall, 
        c("network", "parallelized"), 
        summarise, count = length(overallRuntime), 
        mean = mean(overallRuntime), var = var(overallRuntime)
  )

### Print result
runtime_CA_summary    

runtime_overall_summary


####################
## t-TEST on runtime difference
####################
amazon <- runtime[runtime$network == 'amazon', ]
dblp <- runtime[runtime$network == 'dblp', ]
lj <- runtime[runtime$network == 'lj', ]
youtube <- runtime[runtime$network == 'youtube', ]

t.test(amazon$CA_runtime~amazon$parallelized)
t.test(dblp$CA_runtime~dblp$parallelized)
t.test(lj$CA_runtime~lj$parallelized)
t.test(youtube$CA_runtime~youtube$parallelized)

t.test(runtime_overall[runtime_overall$network == "amazon", ]$overallRuntime~
         runtime_overall[runtime_overall$network == "amazon", ]$parallelized)
t.test(runtime_overall[runtime_overall$network == "dblp", ]$overallRuntime~
         runtime_overall[runtime_overall$network == "dblp", ]$parallelized)
t.test(runtime_overall[runtime_overall$network == "lj", ]$overallRuntime~
         runtime_overall[runtime_overall$network == "lj", ]$parallelized)
t.test(runtime_overall[runtime_overall$network == "youtube", ]$overallRuntime~
         runtime_overall[runtime_overall$network == "youtube", ]$parallelized)
