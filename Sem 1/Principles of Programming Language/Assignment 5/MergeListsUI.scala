// Removing all duplicates using union & intersection
import scala.io.StdIn
object MergeListsUI{
  def main(args: Array[String]): Unit ={
    val list1 = List(1, 2, 3, 4)
    val list2 = List(3, 4, 5, 6)

    println("List 1: " + list1)
    println("List 2: " + list2)

    print("Enter a new element: ")
    val element = StdIn.readInt()

    val union = list1 union list2
    val intersection = list1 intersect list2
    val merged = union :+ element
    val result = merged.filterNot(intersection.contains)

    println("Union: " + union)
    println("Intersection: " + intersection)
    println("Final List: " + result)
  }
}
