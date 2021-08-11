#91000
активация сюрприза~
2 c 100
сюрприз~
eval victim %arg.id%
attach 91001 %victim%
exec 91001 %victim%
~
#91001
сюрприз~
0 abz 33
~
пук
~
#91002
буст~
2 c 100
буст~
%send% %actor% вы почувствовали мегамощь
eval str %actor.stradd(+100)%
eval dex %actor.dex(100)%
eval con %actor.con(100)%
eval wis %actor.wis(100)%
eval int %actor.int(100)%
eval cha %actor.cha(100)%
%send% %actor% %str% %dex% %con% %wis% %int% %cha%
~
$~
