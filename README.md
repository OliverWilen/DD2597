# DD2597
Project Repository for Group 4

### Collaborators: 
* Oliver Wilén 
* Marco Godow
* Sebastian Veijalainen 
* Marcus Dypbukt Källman
# Vulnerabilities
## Extended Data leak
The memory leak attack was extended by allocating an int in the monitor that should not be accessible for the driver - but by finding out the physical address of the int and using it as the address for the physical buffer in virtio_to_queue, it was leaked and visible when dumping traffic from qemu and opening the dump in Wireshark. The following command was used: 

> qemu-system-i386 --enable-kvm -m 256 -hda minix_x86.img -nic user,id=u1,model=virtio-net-pci -object filter-dump,id=f1,netdev=u1,file=/<PATH_TO_DUMPFILE/DUMPFILE.dat>

(The physical address was, so far, found out by printing it and hard coding it in the driver - if trying to reproduce this, check the printed address and that it matches the one in the driver, otherwise something else will be queued for transmission by the driver).

Below is a screenshot showing the printout of the secret variable and a captured packet (both showing a hexadecimal value).
![scr](https://user-images.githubusercontent.com/94693603/218114740-40647548-f8d3-4502-8857-dccc44bd1854.png)



