TEMPLATE = subdirs


symbian {
    contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2)|contains(S60_VERSION, 5.0) {
        SUBDIRS += 3_1 3_2
    }

    contains(S60_VERSION, 5.0)|contains(S60_VERSION, symbian3) {
        SUBDIRS += feedback
    }

    # 5.0 is used also for Symbian3 and later
    SUBDIRS += 5_0
}