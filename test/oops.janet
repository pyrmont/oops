(import ../build/oops)

(def T (oops/emit-type "shopping-list" [:items]))

(print (type T))

(def val (oops/emit-instance T [["eggs"]]
           {:tostring (fn [] "hello world")}))

(print (type val))
(print val)

(pp (val :items))

(put val :items ["apples" "pears"])

(pp (val :items))

(def P (oops/emit-protocol "append"
         [{:append {:doc "Appends x to coll"
                    :args ['coll 'x]}}]))

(print (type P))
