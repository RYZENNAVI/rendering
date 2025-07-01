# ✅ Secure and Clean C Programming – Checklist

## ✅ General Code Quality
- [ ] Functions are small and focused on a single task.
- [ ] Descriptive and meaningful names for variables and functions.
- [ ] Comments are clear and purposeful (no commented-out code clutter).
- [ ] Consistent code style (e.g., enforced via `clang-format`).

## 🛡️ Security Practices
- [ ] Avoid buffer overflows:
  - Never use `gets()`, unbounded `strcpy()` or `sprintf()`.
  - Prefer `fgets()`, `strncpy()`, `snprintf()`.
- [ ] Check for integer overflows, especially during memory allocations.
- [ ] Avoid format string vulnerabilities:
  - Never pass user input directly to `printf()`-style functions.
- [ ] Always validate external input (user, file, network).
- [ ] Check pointers for `NULL` before dereferencing.
- [ ] Avoid use-after-free:
  - Set pointers to `NULL` after calling `free()`.
- [ ] Prevent memory leaks:
  - Every `malloc()` or `calloc()` must be matched with a `free()`.
- [ ] Avoid large local (stack) variables – use heap if needed.
- [ ] Always check system call return values (`open()`, `read()`, `write()`, etc.).
- [ ] Set proper file permissions on creation (`open(..., 0666)`).

## 🧪 Testing & Debugging
- [ ] Write unit tests (e.g., with `cmocka`, `Unity`, or custom test code).
- [ ] Use static analysis tools (`clang-tidy`, `cppcheck`, `splint`).
- [ ] Use Valgrind to check for memory leaks and invalid access.
- [ ] Use `assert()` to verify internal program invariants.
- [ ] Test edge cases thoroughly (null pointers, boundary values, etc.).

## 🧠 Maintainability & Portability
- [ ] Avoid magic numbers – use `#define` or `const` variables.
- [ ] Write platform-independent code (avoid system-specific headers).
- [ ] Always compile with warnings enabled:
  - `-Wall -Wextra -Wpedantic`
- [ ] Provide consistent and helpful error reporting (`perror()`, `strerror()`).

## ⚙️ Build & Deployment
- [ ] Use a `Makefile` or build system with clear targets (build, clean, test).
- [ ] Enable compiler security flags:
  - `-Wall -Wextra -Werror`
  - `-fsanitize=address`
  - `-D_FORTIFY_SOURCE=2` (with `-O2`)
  - `-fstack-protector-strong`
- [ ] Never hardcode sensitive data (passwords, tokens, keys).

## 🔐 Optional Hardening
- [ ] Enable stack canaries.
- [ ] Use RELRO, PIE, DEP via linker flags:
  - `-Wl,-z,relro`, `-Wl,-z,now`, `-fPIE`, `-pie`
- [ ] Consider sandboxing mechanisms (e.g., `seccomp`, `chroot`, containers).

---

**Tip:** 
- Add Danies19 and your Teamlead to your merge Requests as a Reviewer. 
- If you have any questions or tips, reach out to danies19 / Kuriboh (via Discord.)
