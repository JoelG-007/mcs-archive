object Palindrome {
  def main(args: Array[String]): Unit ={
    val list = List(1, 2, 3, 2, 1)

    var palindrome = true
    var i = 0
    var j = list.length - 1

    while (i < j) {
      if (list(i) != list(j)){
        palindrome = false
      }

      i += 1
      j -= 1
    }

    if (palindrome)
      println("The list is a palindrome")
    else
      println("The list is not a palindrome")
  }
}