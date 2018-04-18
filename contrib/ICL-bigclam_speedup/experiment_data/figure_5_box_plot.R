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
## FIGURE 6 - Runtime comparison between parallelized and unparallelized compuation
####################
# Amazon product co-purchase network
amazon <- runtime[runtime$network == 'amazon', ]

pdf("./figures/bigclam_speedup_comparison_amazon.pdf", width = 3, height = 5)
ggplot() + theme_bw() +
  geom_boxplot(data = amazon, 
               aes(x = parallelized, 
                   y = CA_runtime)) +
  scale_y_continuous(limits = c(0, max(amazon$CA_runtime) * 1.1)) +
  xlab("parallelized computation") + 
  ylab("Time taken (s) - community association stage") +
  theme(axis.title.x = element_text(vjust = -0.5),
        axis.title.y = element_text(vjust = 1))
dev.off()

# DBLP collaboration network
dblp <- runtime[runtime$network == 'dblp', ]

pdf("./figures/bigclam_speedup_comparison_dblp.pdf", width = 3, height = 5)
ggplot() + theme_bw() +
  geom_boxplot(data = dblp, 
               aes(x = parallelized, 
                   y = CA_runtime)) +
  scale_y_continuous(limits = c(0, max(dblp$CA_runtime) * 1.1)) +
  xlab("parallelized computation") + 
  ylab("Time taken (s) - community association stage") +
  theme(axis.title.x = element_text(vjust = -0.5),
        axis.title.y = element_text(vjust = 1))
dev.off()

# LiveJournal online social network
lj <- runtime[runtime$network == 'lj', ]

pdf("./figures/bigclam_speedup_comparison_lj.pdf", width = 3, height = 5)
ggplot() + theme_bw() +
  geom_boxplot(data = lj, 
               aes(x = parallelized, 
                   y = CA_runtime)) +
  scale_y_continuous(limits = c(0, max(lj$CA_runtime) * 1.1)) +
  xlab("parallelized computation") + 
  ylab("Time taken (s) - community association stage") +
  theme(axis.title.x = element_text(vjust = -0.5),
        axis.title.y = element_text(vjust = 1))
dev.off()

# Youtube online social network
youtube <- runtime[runtime$network == 'youtube', ]

pdf("./figures/bigclam_speedup_comparison_youtube.pdf", width = 3, height = 5)
ggplot() + theme_bw() +
  geom_boxplot(data = youtube, 
               aes(x = parallelized, 
                   y = CA_runtime)) +
  scale_y_continuous(limits = c(0, max(youtube$CA_runtime) * 1.1)) +
  xlab("parallelized computation") + 
  ylab("Time taken (s) - community association stage") +
  theme(axis.title.x = element_text(vjust = -0.5),
        axis.title.y = element_text(vjust = 1))
dev.off()