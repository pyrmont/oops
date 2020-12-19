(import ../build/oops)

# Define a type
(def T (oops/emit-type
         "shopping-list"
         [:items]
         {:tostring (fn [self]
                      (string "My shopping list is:"
                              (reduce |(string $0 "\n[ ] " $1) "" (self :items))))
          :compare  (fn [lhs rhs]
                      (let [lhs-items (lhs :items)
                            rhs-items (rhs :items)]
                        (if (= lhs-items rhs-items)
                          0
                          (if (<= (length lhs-items) (length rhs-items))
                             1
                             -1))))}))

# Print the type of T
(print "The type of T is " (type T))

# Initialise an instance of T
(def val (oops/emit-instance T [["eggs"]]))
(print "The type of val is " (type val))
(printf "The value of :items is %j" (val :items))
(print val)

# Change a field's data
(put val :items ["apples" "pears"])
(printf "The value of :items is %j" (val :items))
(print val)

(def other (oops/emit-instance T [["apples" "pears"]]))
(print "The comparison of val to other returns " (= val other))

# (def P (oops/emit-protocol "append"
#          [{:append {:doc "Appends x to coll"
#                     :args ['coll 'x]}}]))
# (print (type P))
