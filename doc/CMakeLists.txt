
# Documentation
if (DOXYGEN_FOUND AND GFX_BUILD_DOC)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")
    file(GLOB DOCFILES *.md)
    doxygen_add_docs(gfx_doc
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/README.md
        ${DOCFILES}
        ALL)
endif()