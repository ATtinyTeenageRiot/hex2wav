#ifndef DUKTAPE_FILEIO_H
#define DUKTAPE_FILEIO_H

//#if defined(DUK_CMDLINE_FILEIO)
static duk_ret_t fileio_read_file(duk_context *ctx) {
    const char *fn;
    char *buf;
    size_t len;
    size_t off;
    int rc;
    FILE *f;

    fn = duk_require_string(ctx, 0);
    f = fopen(fn, "rb");
    if (!f) {
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "cannot open file %s for reading, errno %ld: %s",
                  fn, (long) errno, strerror(errno));
    }

    rc = fseek(f, 0, SEEK_END);
    if (rc < 0) {
        (void) fclose(f);
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "fseek() failed for %s, errno %ld: %s",
                  fn, (long) errno, strerror(errno));
    }
    len = (size_t) ftell(f);
    rc = fseek(f, 0, SEEK_SET);
    if (rc < 0) {
        (void) fclose(f);
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "fseek() failed for %s, errno %ld: %s",
                  fn, (long) errno, strerror(errno));
    }

    buf = (char *) duk_push_fixed_buffer(ctx, (duk_size_t) len);
    for (off = 0; off < len;) {
        size_t got;
        got = fread((void *) (buf + off), 1, len - off, f);
        if (ferror(f)) {
            (void) fclose(f);
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "error while reading %s", fn);
        }
        if (got == 0) {
            if (feof(f)) {
                break;
            } else {
                (void) fclose(f);
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "error while reading %s", fn);
            }
        }
        off += got;
    }

    if (f) {
        (void) fclose(f);
    }

    return 1;
}

static duk_ret_t fileio_write_file(duk_context *ctx) {
    const char *fn;
    const char *buf;
    size_t len;
    size_t off;
    FILE *f;

    fn = duk_require_string(ctx, 0);
    f = fopen(fn, "wb");
    if (!f) {
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "cannot open file %s for writing, errno %ld: %s",
                  fn, (long) errno, strerror(errno));
    }

    len = 0;
    buf = (char *) duk_to_buffer(ctx, 1, &len);
    for (off = 0; off < len;) {
        size_t got;
        got = fwrite((const void *) (buf + off), 1, len - off, f);
        if (ferror(f)) {
            (void) fclose(f);
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "error while writing %s", fn);
        }
        if (got == 0) {
            (void) fclose(f);
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "error while writing %s", fn);
        }
        off += got;
    }

    if (f) {
        (void) fclose(f);
    }

    return 0;
}
//#endif  /* DUK_CMDLINE_FILEIO */


#endif // DUKTAPE_FILEIO_H
