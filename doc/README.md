# Documentation

Old project's README files and man pages are stored in the [archive](archive) directory.

## Man pages
- [English](en)
- [日本語](ja)

Manual pages for oneko are written in Perl POD format. To generate roff output
using pod2man, run the command below.

```sh
lang=en # assuming you want to generate english man pages
pod2man -u --lquote='``' --rquote="''" \
  -s 6 -n oneko -r 1.2.sakura.5 -c 'oneko manual' \
   <$lang/README.pod >oneko.man
```

Run `man ./oneko.man` to view the generated man page in your terminal emulator.
