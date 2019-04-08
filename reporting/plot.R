library(tidyverse)

df = read.csv('results.csv')
df

ggplot(df, aes(x=graph, y=proc)) +
  geom_point(aes(col=metric)) +
  geom_line(aes(col=metric)) +
  scale_y_log10() +
  xlab("Graph500 instance") +
  ylab("Execution time [s]") +
  ggtitle("SuiteSparse:GraphBLAS execution times on Graph500 instances") +
  theme_bw() +
  theme(legend.position = "bottom", legend.title = element_blank())

ggsave(file="results.pdf", width=250, height=170, units="mm")
