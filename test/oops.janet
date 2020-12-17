(import ../build/oops)

(def T (oops/emit-type "shopping-list" [:items]))

(print (type T))

(def val (oops/emit-instance T [["eggs"]]))

(print (type val))

(pp (val :items))
