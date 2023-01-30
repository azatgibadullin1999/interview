#!/bin/sh
module="larlena_module_pipe"
device="ft_pipe"
mode="666"
# вызвать insmod со всеми полученными параметрами
# и использовать имя пути, так как новые modutils не просматривают . по
# умолчанию
/sbin/insmod ./$module.ko $* || exit 1
# удалить давно ненужные узлы
rm -f /dev/${device}[0-3]
major=$(awk "\$2==\"$device\" {print \$1}" /proc/devices)
mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3
# назначьте соответствующую группу/разрешения, и измените группу.
# не все дистрибутивы имеют "staff", некоторые вместо этого используют
# "wheel".
group="staff"
grep -q '^staff:' /etc/group || group="wheel"
chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]