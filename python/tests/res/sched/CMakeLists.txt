set(TEST_SOURCES
    __init__.py
    test_sched.py
)

add_python_package("python.tests.res.sched" ${PYTHON_INSTALL_PREFIX}/tests/res/sched "${TEST_SOURCES}" False)

if (STATOIL_TESTDATA_ROOT)
  addPythonTest(tests.res.sched.test_sched.SchedFileTest LABELS StatoilData)
endif()
