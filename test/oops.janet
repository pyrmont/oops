(import ../build/oops)

(def T (oops/emit-type "abc"))

(print (type T))

(def val (oops/emit-instance T))

(print (type val))
