#91000
��������� ��������~
2 c 100
�������~
eval victim %arg.id%
attach 91001 %victim%
exec 91001 %victim%
~
#91001
�������~
0 abz 33
~
���
~
#91002
����~
2 c 100
����~
%send% %actor% �� ������������� ��������
eval str %actor.stradd(+100)%
eval dex %actor.dex(100)%
eval con %actor.con(100)%
eval wis %actor.wis(100)%
eval int %actor.int(100)%
eval cha %actor.cha(100)%
%send% %actor% %str% %dex% %con% %wis% %int% %cha%
~
$~
