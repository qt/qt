import Qt 4.6

import "local"

// WRONG - cannot use qualification to access "local sublibraries".
// (would have to import lib.SubLib)
SubLib.Bar {
}
