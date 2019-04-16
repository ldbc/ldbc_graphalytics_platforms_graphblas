library(tidyverse)

df = read.csv('results.csv')
df = df[df$graph_set == "graph500", ]

nice_y_axis = function() {
  options(scipen=999)
  
  # y axis labels
  longticks = c(T, T, F, T, F, F, F, F, T)
  shortticks = 2:10
  range = -6:6
  
  ooms = 10^range
  
  ybreaks = as.vector(shortticks %o% ooms)
  ylabels = as.character(ybreaks * longticks)
  ylabels = gsub("^0$", "", ylabels)
  
  list(ybreaks = ybreaks, ylabels = ylabels)
}
yaxis <- nice_y_axis()

ggplot(df, aes(x=graph, y=proc)) +
  geom_point(aes(col=tool)) +
  geom_line(aes(col=tool)) +
  scale_y_log10(breaks = yaxis$ybreaks, labels = yaxis$ylabels) +
  xlab("Graph500 instance") +
  ylab("Execution time [s]") +
  ggtitle("SuiteSparse:GraphBLAS execution times on Graph500 instances") +
  facet_wrap(~metric, drop=FALSE, scales="free_y") +
  theme_bw() +
  theme(legend.position = "bottom", legend.title = element_blank())

ggsave(file="graphalatytics-suitesparse-results.pdf", width=250, height=170, units="mm")
