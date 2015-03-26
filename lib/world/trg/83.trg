* BRusMUD trigger file v1.0
#8300
Greeting~
0 r0 100
~
if (%actor.eq(тело)%) = 8302 
  mecho Прораб здоровается с тобой: Привет, деньги в сундуке, никому не нужны! 
  mecho Забери ты их скорее
else  
  mecho Прораб машет вам рукой: Проезжайте, нечего ват тут делать!
  mforce %actor% %-direction%
end




~
#8301
Key_receiver~
0 j0 0
~
if %object% == 8300
wait 1
mecho 'Спасибо за подарок. Теперь я смогу открыть этот ящик'
mexp %actor% 10000

































~
#8302
Door_close~
2 0 0
~
wdoor 8316 запад cd
wdoor 8318 восток cd

































~
#8303
Door_open~
0 0 0
~

wdoor 8316 запад purge
wdoor 8318 восток purge

































~
#8312
speech~
0 q0 0
~
msay Ходят и ходят, клад небось ищут.

























~
$
$
