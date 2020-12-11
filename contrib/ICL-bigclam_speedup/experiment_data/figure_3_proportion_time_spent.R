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
## FIGURE 3 - Proportion of time spent in each stage for different networks
####################
runtime_unparallelized_melted <-
  melt(runtime[runtime$parallelized == FALSE, ],
       id.vars = c("network"), 
       measure.vars = c("CT_runtime", "GA_runtime",
                        "CA_runtime")
      )

runtime_unparallelized_summary <- 
  ddply(runtime_unparallelized_melted, 
        c("network", "variable"), 
        summarise, mean = mean(value)
       )

pdf("./figures/bigclam_speedup_runtime_np_summary.pdf", width = 6, height = 4)       
ggplot(runtime_unparallelized_summary, 
       aes(x = network, y = mean, fill = variable)) + 
  theme(legend.position="bottom") +
  theme_bw() +
  geom_bar(position = position_fill(reverse=TRUE), stat = "identity") + 
  scale_y_continuous(labels = percent_format()) +
  ylab("Proportion of time spent in stage") + xlab("network") +
  coord_flip() + 
  theme(legend.position = "bottom", legend.direction = "horizontal",
        axis.title.x = element_text(vjust = -0.5),
        axis.title.y = element_text(vjust = 1)) +
  scale_fill_discrete(name = "Stage",
                      breaks = c("CT_runtime", "GA_runtime",
                                 "CA_runtime"),
                      labels = c("Conductance Test", "Gradient Ascent",
                                 "Community Association")     
                     )
dev.off()
