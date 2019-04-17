#!/usr/bin/Rscript
most = 2
least = 0.5
high_quantile = 0.9
low_quantile = 0.1
data = read.csv('./data.csv', header=T)  ##读入标准数据
# colnames(data) = c("mapid", "adxcode", "regioncode", "time", "win","weights")
# data$adxcode = sample(1:10,length(data[,1]),replace = T)
# data$mapid = as.factor(data$mapid)
data$adxcode = as.factor(data$adxcode)
    data$regioncode = as.factor(data$regioncode)
    data$regioncode = relevel(data$regioncode,ref = "1156110000") ##北京设置为基准
    data$time = as.factor(data$time)
    data$time = relevel(data$time,ref = "12") ##12点设置为基准
    data$weights = as.numeric(data$weights)
#逻辑回归算系数
    model = glm(win~adxcode+regioncode+time, family = binomial,  weights=weights,data=data)
    d = as.data.frame(coefficients(model))  ##提取变量系数
    d = as.data.frame(cbind(rownames(d),coefficients(model)))
##region
    region = d[grep(pattern="regioncode",d[,1]),]
    region[,1] = substring(region[,1],15,20) ##去掉regioncode1156
    colnames(region) = c("region_code","coefficient")
    region$region_code = as.numeric(region$region_code)
    df_r = data.frame("region_code"=110000,"coefficient"="0") ##添加基准的系数为0
    region = rbind(region,df_r)
    region$coefficient = as.numeric(levels(region$coefficient)[region$coefficient])
    region$coefficient = region$coefficient-min(region$coefficient) ##系数全部变为正值
    region$adjust = region$coefficient/max(region$coefficient)
    region$adjust[which(region$adjust<=0.1)]=0.1  ##避免之后的除法超界
    region$adjust = 1/region$adjust #系数大的更容易买到，提价减少
    upper = quantile(region$adjust,high_quantile)
    lower = quantile(region$adjust,low_quantile)
    region$adjust[which(region$adjust>=upper)]=upper
    region$adjust[which(region$adjust<=lower)]=lower
    region$adjust = ((region$adjust-lower)/(upper-lower))*(most-least)+least  ##将调价倍数放缩到0.5-2之间
    result_region = region[,c(1,3)]
    write.table(result_region,"result_region.txt",row.names = F,sep = "\t") ##写出结果
##time
    time = d[grep(pattern="time",d[,1]),]
    time[,1] = substring(time[,1],5,6)
    colnames(time) = c("hour","coefficient")
    df_h = data.frame("hour"=24,"coefficient"="0")
    hour = rbind(time,df_h)
    hour[,2] = as.numeric(levels(hour$coefficient)[hour$coefficient])
    hour$coefficient = hour$coefficient-min(hour$coefficient)  ##系数全部变为正值
    hour$adjust = hour$coefficient/max(hour$coefficient)
    hour$adjust[which(hour$adjust<=0.1)]=0.1
    hour$adjust = 1/hour$adjust
    upper = quantile(hour$adjust,high_quantile)
    lower = quantile(hour$adjust,low_quantile)
    hour$adjust[which(hour$adjust>=upper)]=upper
    hour$adjust[which(hour$adjust<=lower)]=lower
    hour$adjust = ((hour$adjust-lower)/(upper-lower))*(most-least)+least
    result_hour = hour[,c(1,3)]
    write.table(result_hour,"result_hour.txt",row.names = F,sep="\t")
##adxcode
    adxcode = d[grep(pattern="adxcode",d[,1]),]
    adxcode[,1] = substring(adxcode[,1],8,9) ##去掉adxcodecode
    colnames(adxcode) = c("adxcode_code","coefficient")
    adxcode$adxcode_code = as.numeric(adxcode$adxcode_code)
    df_r = data.frame("adxcode_code"=1,"coefficient"="0") ##添加基准adxcode 1的系数为0
    adxcode = rbind(adxcode,df_r)
    adxcode$coefficient = as.numeric(levels(adxcode$coefficient)[adxcode$coefficient])
    adxcode$coefficient = adxcode$coefficient-min(adxcode$coefficient) ##系数全部变为正值
    adxcode$adjust = adxcode$coefficient/max(adxcode$coefficient)
    adxcode$adjust[which(adxcode$adjust<=0.1)]=0.1  ##避免之后的除法超界
    adxcode$adjust = 1/adxcode$adjust #系数大的更容易买到，提价减少
    upper = quantile(adxcode$adjust,high_quantile)
    lower = quantile(adxcode$adjust,low_quantile)
    adxcode$adjust[which(adxcode$adjust>=upper)]=upper
    adxcode$adjust[which(adxcode$adjust<=lower)]=lower
    adxcode$adjust = ((adxcode$adjust-lower)/(upper-lower))*(most-least)+least  ##将调价倍数放缩到0.5-2之间
    result_adxcode = adxcode[,c(1,3)]
    write.table(result_adxcode,"result_adxcode.txt",row.names = F,sep = "\t") ##写出结果

