#prepare data so that only interesting period shows up 
mydata = read.csv2 ("Desktop/LOG.TXT")

#Stein Hart Coefficients
A=1.127005602e-3
B=2.344242532e-4
C=0.868836922e-7


#userinput to align measuremnts with time info
starttimeChar <- readline(prompt="Enter Start Time: ")
startdateChar <- readline(prompt="Enter Start Date: ")
endtimeChar <- readline(prompt="Enter End Time: ")
enddateChar <- readline(prompt="Enter End Date: ")

startChar <- paste(startdateChar,starttimeChar)
endChar <- paste(enddateChar,endtimeChar)

startmoment <- strptime(startChar, "%d/%m/%y %H%M")
endmoment <- strptime(endChar, "%d/%m/%y %H%M")

mylength <- nrow(mydata)
runtime <- as.numeric(difftime(endmoment,startmoment,units=c("secs")))
incrsteps <- runtime/mylength


ii=0
while (ii<=mylength) {
   mydata[ii,3]=as.character(startmoment)
  startmoment=startmoment+incrsteps
  
  testvalue=mydata[ii,1]
  #calculate voltage that the adc pin sees
  vadc = (testvalue * 2.56) /1024
  #calculate resistance
  resistance=(3.3*10000/vadc)-10000
  #calculate temperature with steinhart formula and above coefficients
  Temp_reading= 1/(A+B*log(resistance)+C*(log(resistance))^3) - 273.15
  mydata[ii,4]=Temp_reading
  
  ii=ii+1
}

mytimestamp=as.POSIXct(mydata[,3],format="%Y-%m-%d %H:%M:%S") #convert timeinfo to posix format as it is needed in that format by ggplot

  library(ggplot2)
  #legend("topleft", legend = c("Light","Temperature"),col=c("green","blue"), lty=1:1, cex=0.8)
  
  p<-ggplot()
  p<- p+geom_line(data=mydata,aes(y=(mydata[,4]*10),x=mytimestamp, color="Temperature"))
  p<- p+geom_line(data=mydata,aes(y=mydata[,2],x=mytimestamp, color="Light"))+ylab("Light [adc]")
  p<- p+ scale_colour_manual(name="Parameters",values=c("blue","red"))
  
  # now adding the secondary axis, following the example in the help file ?scale_y_continuous
  # and, very important, reverting the above transformation
  p <- p + scale_y_continuous(sec.axis = sec_axis(~./10, name = "Temperature [adc]"))
  p <- p+scale_x_datetime(date_labels = "%d/%m-%H:%M")+xlab("Time of day")
  p
  
       