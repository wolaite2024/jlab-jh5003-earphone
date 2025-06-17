 AREA   TESTROOT, DATA, READONLY, PREINIT_ARRAY
        EXPORT pTestRoot
        EXPORT pTestRoot_end

pTestRoot
        INCBIN  .\TestCA.p7b
pTestRoot_end
        END
