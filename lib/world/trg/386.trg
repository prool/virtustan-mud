* BRusMUD trigger file v1.0
#38600
подходим к злому мобу ~
0 q0 100
~
if !(%actor.class% == 8)
встать
kill %actor%
else
wait 1s
   покл %actor.name%.
end










~
#38601
подходим к ползущему~
0 q0 100
~
if !(%actor.class% == 8)
встать
kill %actor%
else
wait 2s
   mecho Безногий скелет попытался поклониться но лишь ткнулся черпом в землю...
end










~
#38602
подходим к дракону~
0 q0 100
~
if !(%actor.class% == 8)
встать
mecho _Костяной дракон воинственно поднял крылья!
kill %actor%
else
wait 1s
   mecho _Костяной дракон вежливо помахал Вам крылами.
end






~
#38603
ноги встречают тело~
0 q0 100
~
if %actor.vnum% == 38601
wait 1s
mecho _Гнилой скелет обрадовался как ребенок при виде своих ног!
calcuid gniloi 38601 mob
mpurge %gniloi.vnum%
wait 1s
mecho _Он приставил костяшки ног к себе и довольно побрел дальше.
mload mob 38604
mpurge %self%
end






~
#38604
тело встречает ноги~
0 q0 100
~
if %actor.vnum% == 38602
wait 1s
mecho _Гнилой скелет обрадовался как ребенок при виде своих ног!
calcuid gniloi 38602 mob
mpurge %gniloi.vnum%
wait 1s
mecho _Он приставил костяшки ног к себе и довольно побрел дальше.
mload mob 38604
mpurge %self%
end






~
#38605
тело встречает ноги~
0 q0 100
~
if %actor.vnum% == 38602
wait 1s
mecho _Гнилой скелет опечалился при виде ног.
say Не мои!
плак
end






~
#38606
подходим к гнилому 2~
0 q0 100
~
if !(%actor.class% == 8)
встать
kill %actor%
else
wait 2s
   mecho _Гнилой скелет склонился перед вами в глубоком реверансе.
end






~
#38607
пурж себя~
0 z0 100
~
mpurge %self%








~
#38608
помер грязный~
0 f0 100
~
if (%world.curobjs(38600)% < 15) && (%random.3% == 1)
mload obj 38600
end

~
#38609
помер гнилой~
0 f0 100
~
if (%world.curobjs(38601)% < 15) && (%random.2% == 1)
mload obj 38601
end

~
$
$
