object Palindrome {
  def main(args: Array[String]): Unit ={
    val list = List(1, 2, 3, 2, 1)

    if (list == list.reverse)
      println("The list is a palindrome")
    else
      println("The list is not a palindrome")
  }
}