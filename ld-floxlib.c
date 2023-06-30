/*
 * ld-floxlib - ld.so hack allowing Nix binaries to impurely
 *              load RHEL system libraries as last resort
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif  /* _GNU_SOURCE */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <link.h>
#include <sys/stat.h>

static int audit_impure = -1;
static int debug_ld_floxlib = -1;
static char name_buf[PATH_MAX];

unsigned int
la_version(unsigned int version)
{
#if 0
    char *ld_audit = getenv("LD_AUDIT");
    char *ld_floxlib_out_path = getenv("LD_FLOXLIB_OUT_PATH");
    if (ld_floxlib_out_path == NULL)
        return version;

    if (debug_ld_floxlib < 0)
        debug_ld_floxlib = (getenv("LD_FLOXLIB_DEBUG") != NULL);

    /*
     * It's sufficient to load ourselves only into the top invoking
     * process and for any subprocesses that get launched to take
     * responsibility for (re)defining LD_AUDIT as required. Moreover,
     * having a Nix-compiled LD_AUDIT library causes dynamic linker
     * problems for native RHEL applications so it's essential that we
     * redact ourselves from the environment of the running process
     * as part of this initial handshake between the dynamic linker
     * and the auditing library.
     */
    int ld_floxlib_strlen = strlen(ld_floxlib_out_path);
    if (debug_ld_floxlib)
        fprintf(stderr, "DEBUG: getenv(LD_AUDIT) = %s\n", ld_audit);
    if (ld_floxlib_strlen == strlen(ld_audit)) {
        // This is the only entry in LD_AUDIT, unset env.
        unsetenv("LD_AUDIT");
        if (debug_ld_floxlib)
            fprintf(stderr, "DEBUG: removing LD_AUDIT\n");
    }
    else {
        char *path_start = strstr(ld_audit, ld_floxlib_out_path);
	if (path_start)
            if (*(path_start + ld_floxlib_strlen) == '\0')
                // ld-floxlib.so at end of LD_AUDIT, replace
                // preceding colon with a null.
                *(path_start - 1) = '\0';
            else
                // ld-floxlib.so at beginning or in middle,
                // copy everything forward to path_start.
                strcpy(path_start, (path_start + ld_floxlib_strlen + 1));
        if (debug_ld_floxlib)
            fprintf(stderr, "DEBUG: getenv(LD_AUDIT) = %s\n", ld_audit);
    }
#endif
    return version;
}

char *
la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag)
{
    struct stat stat_buf;

    if (audit_impure < 0)
        audit_impure = (getenv("LD_FLOXLIB_AUDIT_IMPURE") != NULL);
    if (debug_ld_floxlib < 0)
        debug_ld_floxlib = (getenv("LD_FLOXLIB_DEBUG") != NULL);

    if (debug_ld_floxlib)
        fprintf(stderr, "DEBUG: la_objsearch: %s\n", name);

    if (flag == LA_SER_DEFAULT && stat(name, &stat_buf) != 0) {
        char *basename = strrchr(name, '/');
        char *flox_env = getenv("FLOX_ENV");

        if (basename != NULL)
            basename++;
        else
            basename = (char *) name;

        if (debug_ld_floxlib)
            fprintf(stderr, "DEBUG: looking for: %s\n", basename);

#ifdef LD_FLOXLIB_LIB
        // First attempt to find the lib in the LD_FLOXLIB_LIB
        // cache of common libraries.
        (void) snprintf(name_buf, sizeof(name_buf), "%s/%s", LD_FLOXLIB_LIB, basename);
        if (debug_ld_floxlib)
            fprintf(stderr, "DEBUG: checking: %s\n", name_buf);
        if (stat(name_buf, &stat_buf) == 0) {
            if (audit_impure)
                fprintf(stderr, "AUDIT: %s -> %s\n", name, name_buf);
            return name_buf;
        }
#endif

        // Finally look for the lib in $FLOX_ENV/lib.
        if (flox_env != NULL) {
            (void) snprintf(name_buf, sizeof(name_buf), "%s/lib/%s", flox_env, basename);
            if (debug_ld_floxlib)
                fprintf(stderr, "DEBUG: checking: %s\n", name_buf);
            if (stat(name_buf, &stat_buf) == 0) {
                if (audit_impure)
                    fprintf(stderr, "AUDIT: %s -> %s\n", name, name_buf);
                return name_buf;
            }
        }
    }

    return (char *) name;
}

/* vim: set et ts=4: */
