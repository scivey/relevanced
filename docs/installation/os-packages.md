# OS Packages

## Ubuntu DEB package

The easiest way to install relevanced on Ubuntu is with the prebuilt `.deb` binary package.  Note that this package is only tested on Ubuntu 14.04 (Trusty) and above, and may not work on lower versions.

The latest `.deb` package can always be found on the [GitHub release page](https://github.com/scivey/relevanced/releases).

You can install the downloaded `.deb` file using `dpkg` and `apt` on the command line:
```bash
VERSION=0.9.1
sudo dpkg -i relevanced-$VERSION.deb

# installs any missing packages
# encountered by dpkg
sudo apt-get install -f

sudo dpkg -i relevanced-$VERSION.deb
```

Alternately, you can double-click the `.deb` file to install it with Ubuntu's Software Center.

Either of these methods will create:

- The `relevanced` binary (`/usr/bin/relevanced`)
- The default configuration file (`/etc/relevanced/relevanced.json`)
- The default data directory (`/var/lib/relevanced/data`)

## Other platforms
There are plans to support `RPM`-based distros and possibly earlier Ubuntu releases, but this won't happen until after the 1.0 release.
