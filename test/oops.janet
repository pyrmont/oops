(import ../build/oops)

(def T (oops/emit-type
         "shopping-list"
         [:items]
         {:tostring (fn [self]
                      (string "My shopping list is:"
                              (reduce |(string $0 "\n[ ] " $1) "" (self :items))))}))

(print (type T))

(def val (oops/emit-instance T [["eggs"]]))

(print (type val))
(print val)

(pp (val :items))

(put val :items ["apples" "pears"])

(pp (val :items))

(print val)

(def P (oops/emit-protocol "append"
         [{:append {:doc "Appends x to coll"
                    :args ['coll 'x]}}]))

(print (type P))
