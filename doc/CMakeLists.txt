
# Documentation
if (DOXYGEN_FOUND AND GFX_BUILD_DOC)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")
    set(DOXYGEN_RECURSIVE NO)
    file(GLOB DOCFILES *.md)
    doxygen_add_docs(gfx_doc
        ${HEADERS}
        ${PROJECT_SOURCE_DIR}/README.md
        ${DOCFILES})
endif()