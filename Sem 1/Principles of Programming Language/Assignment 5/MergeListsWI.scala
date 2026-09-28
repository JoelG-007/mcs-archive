// Removing only duplicates with user entry
import scala.io.StdIn

object MergeLists {
  def main(args: Array[String]): Unit = {

    print("Enter number of elements for List 1: ")
    val n1 = StdIn.readInt()
    var list1 = List[Int]()

    var i = 0
    while (i < n1) {
      print("Enter element " + (i + 1) + ": ")
      val x = StdIn.readInt()
      list1 = list1 :+ x
      i += 1
    }

    print("Enter number of elements for List 2: ")
    val n2 = StdIn.readInt()
    var list2 = List[Int]()

    i = 0
    while (i < n2) {
      print("Enter element " + (i + 1) + ": ")
      val x = StdIn.readInt()
      list2 = list2 :+ x
      i += 1
    }

    println("List 1: " + list1)
    println("List 2: " + list2)

    print("Enter a new element: ")
    val element = StdIn.readInt()
    val merged = list1 ++ list2 :+ element

    println("Before Removing Duplicates: " + merged)

    val finalList = merged.distinct

    println("Final List: " + finalList)
  }
}
