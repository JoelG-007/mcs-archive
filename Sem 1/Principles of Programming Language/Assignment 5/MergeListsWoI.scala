// Removing only duplicates without user entry
object MergeLists {
  def main(args: Array[String]): Unit = {
    val list1 = List(1, 2, 3, 4)
    val list2 = List(3, 4, 5, 6)

    println("List 1: " + list1)
    println("List 2: " + list2)

    print("Enter a new element: ")
    val element = StdIn.readInt()
    val bmerged = (list1 ++ list2 :+ element)
    println("Before Removing Duplicates: "+bmerged)
    val merged = (list1 ++ list2 :+ element).distinct
    println("Final List: " + merged)
  }
}