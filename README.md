# ld-floxlib.so
ld.so hack allowing Nix binaries to impurely load RHEL system libraries as last resort

----
## Background
Ideally, we would migrate to Nix by first porting all software to work in Nix
and then update the default paths to point to the Nix versions. Unfortunately
this plan requires that *everything* must first be ported to Nix before we can
run *anything* in Nix.

As a fallback position, we have updated the python `sitecustomize.py` module to
call out to `bootstrap.py` to load "impure" python modules not found in Nix,
and this has introduced the requirement that dynamic python extensions compiled
on RHEL be able to load RHEL versions of system libraries found in `/lib64`.

----
## Description
The `ld-floxlib.so` plugin works with the GNU dynamic linker auditing API
`LD_AUDIT` variable to search for libraries in the RHEL default `/lib64`
path when they are not found in the Nix default (`/path/to/glibc/lib`)
location.

To be clear, this mechanism exists to defeat the purity guarantees inherent in
Nix and is therefore *A Bad Idea*. However, it also simplifies the process of
migrating to Nix, so we are adding it with the hope and expectation that it
will only be used for a (perhaps extended) transition period while we port all
front-office code to Nix.

----
## Usage
The ld-floxlib customization is enabled by setting the `LD_AUDIT` environment
variable:

    % LD_AUDIT=/path/to/ld-floxlib.so <command and args>

Impure library references can be audited by defining the
`LD_FLOXLIB_AUDIT_IMPURE` environment variable. Auditing lines are prefaced
with the string "AUDIT:" and printed to STDERR:

    % LD_AUDIT=~/src/ld-floxlib/ld-floxlib.so LD_FLOXLIB_AUDIT_IMPURE=1 \
        /tmp/python-std -c 'import deshaw.datetime' > /dev/null
    AUDIT: /nix/store/jc8higknnpzvvx92djv38rprnd5457s2-glibc-2.30/lib/libstdc++.so.6 -> /lib64/libstdc++.so.6
    AUDIT: /nix/store/jc8higknnpzvvx92djv38rprnd5457s2-glibc-2.30/lib/libsnappy.so.1 -> /lib64/libsnappy.so.1

----
## See also
rtld-audit(7)

N.B. "Auditing API" as described in the above manpage is in one regard a
misnomer since it actually allows the dynamic behavior to be changed.
