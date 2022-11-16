# Minix with Dockerfile for building on AArch64

This is a fork of Minix that incorporates [Santurysim's fixes](https://github.com/Stichting-MINIX-Research-Foundation/minix/pull/322) for compiling on newer versions of GCC, and a Dockerfile for running an Ubuntu container which is suitable for compiling Minix
using the build scripts in `releasetools`. I used this Docker container to compile Minix images for i386 on an Apple silicon Macbook (AArch64).
These images can then be run on QEMU obtained from Homebrew.

# Install sw for OSX

1. Install docker via  https://docs.docker.com/desktop/mac/install/
2. Install qemu via brew 

# Getting started

1. Clone this repo `git clone git@gits-15.sys.kth.se:robertog/system-sec-minix.git`
2. Change into the repo: `cd minix`
2. Build the docker image `docker build -t minix-crosscompile -f Dockerfile.build .`
3. Start the container `docker run --name minix-crosscompile-cont -it minix-crosscompile`
4. In the container, build Minix in `/usr/src/` using `./releasetools/x86_hdimage.sh` or another of the build scripts. This takes about 30-45 minutes because it has to compile the entire LLVM/Clang toolchain targeting x86.
6. (optional) Commit your docker container as a new image so that you don't need to recompile the toolchain each time while you develop: `docker commit minix-crosscompile-cont minix-crosscompile:withtools` (make take a bit, the compiled toolchain is a few GB)

# Running minix

1. Copy the built image back to your host machine: `docker cp minix-crosscompile-cont:/usr/src/minix_x86.img /tmp`
2. Run minix in Qemu: ``

# Exposing a IP port to an existing container (Advice: do this at least for SSH)

1. Commit the docker container: `docker commit minix-crosscompile-cont minix-crosscompile:some_branch`
2. Start a new container redirecting the host port x (e.g. 4321) to the
   container port y (e.g. 22 for ssh): `docker run -p 4321:22 --name minix-cont-ssh -it minix-crosscompile:some_branch`

# Accessing container files via SSH

1. In the container, install SSH server: `apt-get install openssh-server`
2. In the container sets up a password for root (or any other authentication
   method for the user you use to build): `echo 'root:pwd' | chpasswd`
3. In the container, if you use password authentication for user root, enable it in the SSH server
   configuration: `sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin
   yes/g' /etc/ssh//sshd_config`
4. In the container start SSH server: `/etc/ini.d/ssh start`
5. Redirect port 4321 to port 22 of the container (see section above)
6. You can now access the container via ssh on port 4321 of your host machine.
   This means that you can use emacs or Visual Studio Code on you host machine,
   you can access the container files via SSH using Tramp or Visual Studio Code
   Remote - SSH. Alternatively you can set up a SSH Fuse file system


# Setting up a new Minix install

I run Minix in a QEMU virtual x86 machine. QEMU may be installed using Homebrew (`brew install qemu`) or another package manager
such as apt (`apt-get install -y qemu-system-x86_64`) or downloaded from [their website](https://www.qemu.org/download/).

When you make a CD image, you can make a fresh install using QEMU as follows:
```sh
qemu-img create -f qcow2 minix.qcow2 10G && \
    qemu-system-x86_64 -cpu max -m 2g \
    -hda minix.qcow2 -cdrom minix_x86.iso \
    -boot d 
```

When you get to the shell prompt in Minix, type **`setup`** and follow the instructions to
install Minix on the blank image, minix.qcow2. Then shutdown with `shutdown -r now`, restart
the QEMU VM without the ISO image mounted 

```sh
qemu-system-x86_64 -cpu max -m 2g \
    -hda minix.qcow2 
```
## Get pkgin and basic packages

For some reason, the 3.4.0. release doesn't have the pkgsrc source repository set correctly.
To get pkgin and then core development packages,
```sh
export PKG_PATH="http://www.minix3.org/pkgsrc/packages/3.4.0/i386/All/"
pkg_add pkgin
pkgin update
pkgin_sets
```

This installs git, vim, ssh, make and other core development utilities.

To activate sshd service, 
```sh
cp /usr/pkg/etc/rc.d/sshd /etc/rc.d/
echo 'sshd=YES' >> /etc/rc.conf
service sshd start
```
And add this line to `/usr/pkg/etc/ssh/sshd_config`:
```
PermitRootLogin yes
```

# Running existing image

The command I use to start Minix in a virtual machine is:
```sh
qemu-system-x86_64 -cpu max -m 2g  \
    -hda minix.qcow2 \
    -netdev user,id=mynet0,hostfwd=tcp::7722-:22 \
    -device e1000,netdev=mynet0
```

This sets up port your host port 7722 to be forwarded to the VM's port 22, so that
you can SSH into the minix instance (`ssh -p 7722 root@localhost`) rather than using the limited functionality QEMU-based terminal.

Once you start the image and it boots, it gives you instructions - type `setup` to set up the package system and other things.

Then make sure the SSH service is activated by running `cp /usr/sbin/sshd /service/`, and set a root password with `passwd`. 
Then you should be able to SSH into your Minix instance from your regular terminal using `ssh -p 7722 root@localhost`.
