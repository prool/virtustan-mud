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
if !%shot%
  eval shot 10
end
calcuid aroom %actor.realroom% room
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval dir %arg1%
eval victim %arg2.id%
if %arg1% == �
  set dir north
elseif %arg1% == �
  set dir south
elseif %arg1% == �
  set dir west
elseif %arg1% == �
  set dir east
elseif %arg1% == ��
  set dir up
elseif %arg1% == ��
  set dir down
else
  %send% %actor% ���������� ����������� ��� ��������
  halt
end
if %shot% < 1
  %send% %actor% �� ���������� �������� �� ��������� ������, �� �� �� ��������� � �����. ������ ������ �����������.
  eval shot 0
  halt
end
if %aroom.%dir%% == nil
  %send% %actor% ��� ���� �������, ���� ���� ����������
  halt
end
eval dir2 %aroom.%dir%%
eval vroom %victim.realroom%
if %dir2% != %vroom%
  %send% %actor% �� �� ����� �������� ����, ���
  halt
else
  calcuid vroom2 %vroom% room
  eval dmg %random.801% +199
  foreach i %vroom2.all%
    if %i% == %victim%
      %send% %victim% �������� �� ����� ������ � ��� �������� ������� ������������ �����
      %damage% %victim% %dmg%
    else
      %send% %i% �������� �� ����� ������ �������� � �������� � %victim.dname% ������� ������������ �����
    done
  end
end
eval shot %shot% -1
global shot
%send% %actor% ����: %victim.name%, �������� �����: %dmg%, �������� �������: %shot% �� 10
~
#91102
����������~
1 c 3
����~
dg_cast %arg%
~
$~
