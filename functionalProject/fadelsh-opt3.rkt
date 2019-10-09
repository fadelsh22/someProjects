#lang racket
(provide (all-defined-out))
(require racket/trace)


(define (weakorder carlist attr1 attr2 attr3)
  (weakorderHelp carlist attr1 attr2 attr3 '() )
  )

(define (weakorderHelp carlist attr1 attr2 attr3 cat)
  (if (null? carlist)
      cat
      (weakorderHelp (cadr (create carlist attr1 attr2 attr3 '() '() carlist))
                   attr1 attr2 attr3
                   (append cat (list (car (create carlist attr1 attr2 attr3 '() '() carlist))))
                                 
                   )
      ))



(define (comparison one two  attr1 attr2 attr3)
 (cond
   [ (equal? one two) 'same]
   [ (compare one two attr1 attr2 attr3) 'one]
   [ (compare two one attr1 attr2 attr3) 'two]
   [ (and (not (compare one two attr1 attr2 attr3)) (not (compare two one attr1 attr2 attr3))) 'not]
   ))


(define (compare one two attr1 attr2 attr3)
     (flipSeq one two attr1 attr2 attr3)
  )

(define (create lst attr1 attr2 attr3 cat cat2 newList)
  (if (null? newList)
       (list cat cat2)
      (if (equal? (isTop lst attr1 attr2 attr3 (car newList)) true)
          (create lst attr1 attr2 attr3 (append cat (list (car newList)) ) cat2 (cdr newList))
          (create lst attr1 attr2 attr3 cat (append cat2 (list (car newList)) ) (cdr newList))
          
           )
      ))

(define (isTop lst attr1 attr2 attr3 temp)
  (if (null? lst)
      true
      (if (equal? (comparison temp (car lst) attr1 attr2 attr3) 'two)
          false
          (isTop (cdr lst) attr1 attr2 attr3 temp)
          )
  ))


(define (flip one two attr1 attr2 attr3)
   (if (and (equal? (cadr one) (cadr two)) (equal? (third one) (third two)) (not (equal? (car one) (car two))))
       (isPreferable (car one) (car two) attr1)
       (if  (and (equal? (car one) (car two)) (equal? (third one) (third two)) (not (equal? (cadr one) (cadr two))))
            (isPreferable (cadr one) (cadr two) attr2)
            (isPreferable (cadr(cdr one)) (cadr(cdr two)) attr3)))
  )


(define (flipSeq one two attr1 attr2 attr3)
  (and (isPreferable (car one) (car two) attr1)
     (isPreferable (cadr one) (cadr two) attr2)
      (isPreferable (cadr(cdr one)) (cadr(cdr two)) attr3)
      )
)

(define (isPreferable one two attr)
  (if (equal? one two)
      true
  (if (null? (find one attr))
      false
      (iterate (find one attr) two attr)
  )))

(define (iterate lst two attr)
  (if (null? lst)
      false
      (if (equal? (car lst) two)
          true
          (if (isPreferable (car lst) two attr)
          true
          (iterate (cdr lst) two attr)))
   ))
  
(define (find one attr)
  (if (null? attr)
      null
      (if (equal? (car (car attr)) one)
          (cadr (car attr))
          (find one (cdr attr)))))
