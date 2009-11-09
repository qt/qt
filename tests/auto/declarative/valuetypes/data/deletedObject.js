var savedReference;

function startup()
{
    savedReference = object.rect;
    print("Test: " + savedReference.x);
}

function afterDelete()
{
    print("Test: " + savedReference.x);
}

