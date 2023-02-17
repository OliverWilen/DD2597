# DD2597
Project Repository for Group 4

### Collaborators: 
* Oliver Wilén 
* Marco Godow
* Sebastian Veijalainen 
* Marcus Dypbukt Källman

## How to start monitor:
Minix should be able to build normally using the releasetools. 
Before starting minix, virtio needs to be used. We use this command to start minix with virtio enabled.
qemu-system-i386 --enable-kvm -m 256 -hda minix_x86.img -nic user,id=u1,model=virtio-net-pci
To activate the monitor, the virtio net driver needs to be used. This is done using netconf.
In the Minix terminal use the netconf command and pick the vi0 driver. Then either restart Minix or the network service.

# Vulnerabilities
## Data Leak Attack
The data leak attack and guide is on the branch sebvei_extend_attack.

## Data Overwrite Attack
The data leak attack and guide is on the branch marcoTest.



