#91100
�������~
2 c 3
/~
wait 1
%arg%
~
#91101
��������������� ��������~
1 c 1
�������~
eval rroom %actor.realroom%
eval arg1 %arg.car%
eval arg2 %arg.words(2)%
switch %arg1.mudcommand%
  case �����
    eval dir %rroom.north%
    set tdir �� �����
    set tdir2 � ���
  break
  case ��
    eval dir %rroom.south%
    set tdir �� ��
    set tdir2 � ������
  break
  case �����
    eval dir %rroom.west%
    set tdir �� �����
    set tdir2 � �������
  break
  case ������
    eval dir %rroom.east%
    set tdir �� ������
    set tdir2 � ������
  break
  case �����
    eval dir %rroom.up%
    set tdir �����
    set tdir2 �����
  break
  case ����
    eval dir %rroom.down%
    set tdir ����
    set tdir2 ������
  break
  default
    %send% %actor% ���������� �����������
    halt
  break
done
if !%dir%
  %send% %actor% � ��������� ����������� ��� �������
  halt
end
foreach i %dir.all%
  if %i.iname% /= %arg2%
    eval victim %i%
    eval vexp %victim.exp%
  break
end
done
if !%victim%
  %send% %actor% �� �� ������ ����
  halt
end
%send% %actor% �� ���������� %tdir% � %victim.vname%
%echoaround% %actor% %actor.iname% ���������%actor.g% %tdir%
%send% %victim% %actor.iname% ���������%actor.g% %tdir2%
oat %dir% %echoaround% %victim% ���-�� ��������� � %victim.vname% %tdir2%
%damage% %victim% 1000
%actor.wait(2)%
if %victim.position% == 0
  eval exp1 %actor.exp%
  %actor.exp(+%vexp%)%
  eval exp2 %actor.exp%-%exp1%
  if %exp2% == 0
    %send% %actor% �� �� �������� �����
  else
    %send% %actor% �������� �����: %exp2% ������
  end
end
~
#91102
����������~
1 c 3
����~
attach 91103 %actor%
exec 91103 %actor%
detach 91103 %actor%
~
#91103
������� ����~
0 z 100
~
dg_cast %arg.car% '%arg.cdr%'
~
$~
