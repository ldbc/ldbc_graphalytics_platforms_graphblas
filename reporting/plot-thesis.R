library(ggplot2)
library(reshape2)
library(RColorBrewer)

# nice_y_axis = function() {
#   options(scipen=999)
#   
#   # y axis labels
#               # 2  3  4  5  6  7  8  9 10
#   longticks = c(F, F, T, F, F, F, T, F, T)
#   shortticks = c(2, 4, 6, 8, 10)
#   range = -6:6
#   
#   ooms = 10^range
#   
#   ybreaks = as.vector(shortticks %o% ooms)
#   ylabels = as.character(ybreaks * longticks)
#   ylabels = gsub("^0$", "", ylabels)
#   
#   list(ybreaks = ybreaks, ylabels = ylabels)
# }
# 
# yaxis = nice_y_axis()
# ybreaks = yaxis$ybreaks
# ylabels = yaxis$ylabels

ybreaks = c(
      0.01,    0.03,
      0.1,     0.3,
       1,      3,
      10,     30,
     100,    300,
    1000,   3000,
   10000,  30000,
  100000, 300000
)

data = read.csv("results-COMPLETE.csv")
#data$type = sapply(strsplit(as.character(data$dataset), "-"), "[", 1)
#levels(data$dataset) = factor(data$dataset)

data$type = apply(data, 1, function(row) {
  types = strsplit(as.character(row[1]), "-")[[1]]
  if (head(types[1]) == "datagen") {
    row[4] = paste(types[1], types[3], sep = "-")
  } else {
    row[4] = paste(types[1])
  }
})

ggplot(data, aes(x=dataset, fill=type)) +
  geom_bar(aes(y=processing), stat="identity") +
  scale_fill_brewer("Dataset category", palette = "Accent") +
  scale_y_log10(breaks=ybreaks, name="Makespan time [s]") +
  scale_x_discrete(name="Dataset") +
  theme_bw() +
  theme(axis.text.x=element_text(angle=40, hjust=1)) +
  facet_wrap(. ~ algorithm, ncol = 1, scales="free_x") +
  #facet_grid(algorithm ~ .) +
  #facet_wrap(~ algorithm, ncol=1, scales="free_x") +
  theme(legend.position="top")

ggsave("results-COMPLETE.pdf", width = 160, height = 250, units = "mm")
